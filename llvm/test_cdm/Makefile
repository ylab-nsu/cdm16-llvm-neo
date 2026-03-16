SRC_DIR = src
VENV_DIR = .venv
INCLUDE_DIR = include
TESTS_DIR = tests
COMMONS_DIR = commons

MAIN_SCRIPT = $(SRC_DIR)/main.py

test: $(VENV_DIR)
	$(VENV_DIR)/bin/python3 $(MAIN_SCRIPT) -I $(INCLUDE_DIR) --cocas cocas $(TESTS_DIR)/*

$(VENV_DIR):
	python3 -m venv $@
	PATH=$(VENV_DIR)/bin:$(VENV_DIR)/Scripts:$$PATH \
	pip install -r requirements.txt

.PHONY: test
