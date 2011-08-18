INTRODUCTION
============

    top-like Beyond the Standard Model (BSM) analysis Input generator.



COMPILE
=======

    The code only works within the CMSSW framework.

        1. git clone <url>
        2. cd bsm_input_maker
        3. git submodule update --init
        4. cd bsm_input
        5. source ./setup.sh
        6. make -j
        7. cd -
        8. scram b -r -j10



USAGE
=====

    Generate input PAT with CRAB:

        1. cd bsm_input_maker/maker/test
        2. mkdir generation
        3. ln -s ../ttbsm_aod_to_pat_cfg.py
        4. ln -s ../crab_data_tape.cfg crab_data.cfg
        5. ln -s /path/to/json/certification/file.txt cert_json.txt
        6. ../crab_data.sh ttbsm_aod_to_pat_cfg.py DATASET cert_json.txt [CMSSW FLAGS]

            where CMSSW flags depend on Input type:
            
                data    useData=1 writeFat=1
                MC      useData=0 writeFat=1    



    Generate PB from PAT:

        [instructions will be added soon]
