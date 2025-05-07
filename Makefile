SHELL := /bin/bash

CC=gcc
CFLAGS=-fopenmp -O2
SEQ=sequencial
PAR=diga_freq
TEST_DIR=./testes
TMP=./tmp

export OMP_NUM_THREADS ?= 4

all: $(SEQ) $(PAR) test

$(SEQ): sequencial.c
	$(CC) -o $(SEQ) sequencial.c

$(PAR): diga_freq.c
	$(CC) $(CFLAGS) -o $(PAR) diga_freq.c

test: $(SEQ) $(PAR)
	@mkdir -p $(TMP)
	@echo "🔍 Iniciando testes..."
	@for in_file in $(TEST_DIR)/*.txt; do \
		base=$$(basename $$in_file .txt); \
		ref_file=$(TEST_DIR)/$$base.out; \
		echo "🔹 Testando $$base..."; \
		./$(SEQ) < $$in_file > $(TMP)/$$base.seq.txt; \
		for i in 1 2 3; do \
			./$(PAR) < $$in_file > $(TMP)/$$base.par$$i.txt; \
			diff -q $(TMP)/$$base.seq.txt $(TMP)/$$base.par$$i.txt || (echo "❌ Falha em $$base.par$$i.txt" && exit 1); \
		done; \
		echo "✅ $$base passou."; \
	done
	@echo "🎉 Todos os testes passaram com sucesso!"

# tamanho bruto de dados (bytes) para cada teste
GEN_SIZE ?= 1000000  
# número de linhas finais (cada linha terá até 1000 chars)
GEN_LINES ?= 10000  
# número de testes a criar
GEN_COUNT ?= 5      

generate:
	@mkdir -p $(TEST_DIR)
	@echo "🔍 Gerando $(GEN_COUNT) arquivos de teste de tamanho ≈$(GEN_SIZE)B..."
	@for i in $$(seq 1 $(GEN_COUNT)); do \
	  echo "🔹 Gerando teste $$i..."; \
	  head -c $(GEN_SIZE) /dev/urandom \
		| tr -dc 'a-zA-Z0-9' \
		| fold -w 1000 \
		| head -n $(GEN_LINES) \
		> $(TEST_DIR)/large_test$$i.txt; \
	done
	@echo "🎉 Testes grandes gerados em $(TEST_DIR): large_test1.txt … large_test$(GEN_COUNT).txt"


output: $(SEQ)
	@echo "📤 Gerando arquivos .out com $(SEQ)..."
	@for in_file in $(TEST_DIR)/*.txt; do \
		base=$$(basename $$in_file .txt); \
		out_file=$(TEST_DIR)/$$base.out; \
		./$(SEQ) < $$in_file > $$out_file; \
		echo "Gerado: $$out_file"; \
	done
	@echo "✅ Todos os arquivos .out foram gerados com sucesso."

clean:
	rm -f $(SEQ) $(PAR)
	rm -rf $(TMP)

.PHONY: bench
bench: $(SEQ) $(PAR)
	@echo "⏱️ Executando benchmark com $$OMP_NUM_THREADS threads..."
	@echo "input,seq_time(s),par_time(s)" > bench.csv
	@for in_file in $(TEST_DIR)/*.txt; do \
		base=$$(basename $$in_file .txt); \
		seq_time=$$( { time -p ./$(SEQ) < $$in_file > /dev/null; } 2>&1 | awk '/^real/ {print $$2}'); \
		par_time=$$( { time -p ./$(PAR) < $$in_file > /dev/null; } 2>&1 | awk '/^real/ {print $$2}'); \
		printf "%s,%s,%s\n" $$base $$seq_time $$par_time >> bench.csv; \
	done
	@echo "✅ Benchmark salvo em bench.csv"