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

    Generate input PAT from AOD with CRAB (data example):

        1. cd bsm_input_maker/maker/test
        2. mkdir generation && cd generation
        3. ln -s ../ttbsm_aod_to_pat_cfg.py
        4. ln -s ../crab_data_tape.cfg crab_data.cfg
        5. ln -s /path/to/json/certification/file.txt cert_json.txt
        6. ../crab_data.sh ttbsm_aod_to_pat_cfg.py DATASET cert_json.txt useData=1 writeFat=1
        7. crab -create
        8. crab -submit

    Generate input PAT from AOD with CRAB (MC example):

        1. see above
        2. see above
        3. see above
        4. ln -s ../crab_mc_tape.cfg crab_mc.cfg
        5. ../crab_mc.sh ttbsm_aod_to_pat_cfg.py DATASET useData=0 writeFat=1
        6. crab -create
        7. crab -submit



    Generate PB from PAT:

        [instructions will be added soon]
