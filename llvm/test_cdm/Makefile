
SRC_DIR = src
VENV_DIR = .venv
INCLUDE_DIR = include
TESTS_DIR = tests
COMMONS_DIR = commons

MAIN_SCRIPT = $(SRC_DIR)/main.py

COMMONS_OBJECTS = $(addprefix $(COMMONS_DIR)/, $(addsuffix .obj, $(basename $(notdir $(wildcard $(COMMONS_DIR)/src/*)))))

all: test

$(VENV_DIR):
	python3 -m venv $@
	$(VENV_DIR)/bin/pip install -r requirements.txt

$(COMMONS_DIR)/src/%.asm: $(COMMONS_DIR)/src/%.c
	$(CLANG) -S -target cdm -O3 -I $(INCLUDE_DIR) -o $@ $^

$(COMMONS_DIR)/%.obj: $(COMMONS_DIR)/src/%.asm
	$(VENV_DIR)/bin/cocas -c -t cdm16 -o $@ $^

test: $(VENV_DIR) $(COMMONS_OBJECTS)
	$(VENV_DIR)/bin/python3 $(MAIN_SCRIPT) -c $(CLANG) -I $(INCLUDE_DIR) $(TESTS_DIR)

.PHONY: test all
