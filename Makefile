all:
	@echo Making all programs.  If this fails, try "'make no_root'"
	@echo to only make programs which don"'"t depend on ROOT.
	@echo ""
	if [ ! -d bin ]; then mkdir bin; fi
	cd add_mca && make
	cd add_scaled_mca && make
	cd add_tree && make
	cd analyze_mca && make
	cd chn2mca && make
	cd contract_mca && make
	cd integrate_mca && make
	cd mca2txt && make
	cd scale_mca && make
	cd spx2mca && make
	cd sum_mca && make
	cd tree2binnedavgtxt && make
	cd tree2mca && make
	cd tree2doppler_map && make
	cd tree2doppler_2Dmap && make
	cd tree2mca_gated && make
	cd tree2mca_group && make
	cd tree2mca_group_2Dmap && make
	cd tree2tree && make
	cd tree_gate_intensity && make
	cd txt2binnedavgtxt && make
	cd txt2mca && make
no_root:
	@echo Making programs which don"'"t depend on ROOT.
	@echo ""
	if [ ! -d bin ]; then mkdir bin; fi
	cd add_mca && make
	cd add_scaled_mca && make
	cd analyze_mca && make
	cd chn2mca && make
	cd contract_mca && make
	cd mca2txt && make
	cd scale_mca && make
	cd spx2mca && make
	cd sum_mca && make
	cd txt2binnedavgtxt && make
	cd txt2mca && make

clean:
	rm bin/*
