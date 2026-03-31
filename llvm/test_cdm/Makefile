SRC_DIR = src
VENV_DIR = .venv
INCLUDE_DIR = include
TESTS_DIR = tests
COMMONS_DIR = commons

MAIN_SCRIPT = $(SRC_DIR)/main.py

COMMONS_OBJECTS = $(addprefix $(COMMONS_DIR)/, $(addsuffix .obj, $(basename $(notdir $(wildcard $(COMMONS_DIR)/src/*)))))

test: $(VENV_DIR) $(COMMONS_OBJECTS)
	PATH=$(VENV_DIR)/bin:$(VENV_DIR)/Scripts:$$PATH \
	python3 $(MAIN_SCRIPT) -c $(CLANG) -I $(INCLUDE_DIR) $(TESTS_DIR)

$(VENV_DIR):
	python3 -m venv $@
	PATH=$(VENV_DIR)/bin:$(VENV_DIR)/Scripts:$$PATH \
	pip install -r requirements.txt

$(COMMONS_DIR)/src/%.asm: $(COMMONS_DIR)/src/%.c
	$(CLANG) -S -target cdm-cocas -O3 -I $(INCLUDE_DIR) -o $@ $^

$(COMMONS_DIR)/%.obj: $(COMMONS_DIR)/src/%.asm
	PATH=$(VENV_DIR)/bin:$(VENV_DIR)/Scripts:$$PATH \
	cocas -c -t cdm16 -o $@ $^


.PHONY: test
