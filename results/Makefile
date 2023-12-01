.SILENT:
.DEFAULT_GOAL:=help

.PHONY: help add_results notebook

venv:
	test -d $(VENV_DIR) || python3 -m venv $(VENV_DIR)

add_results:
	python3 results/add_results.py

notebook:
	jupyter notebook

help:
	echo 'insert/plot/analyze results'
