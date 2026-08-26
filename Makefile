.PHONY: clean All

All:
	@echo "----------Building project:[ manysteps-lib - Release ]----------"
	cd codelite && "$(MAKE)" -f  "manysteps-lib.mk"
	@echo "----------Building project:[ manysteps-unb - Release ]----------"
	cd codelite && "$(MAKE)" -f  "manysteps-unb.mk"
clean:
	@echo "----------Cleaning project:[ manysteps-lib - Release ]----------"
	cd codelite && "$(MAKE)" -f  "manysteps-lib.mk" clean
	@echo "----------Cleaning project:[ manysteps-unb - Release ]----------"
	cd codelite && "$(MAKE)" -f  "manysteps-unb.mk" clean
