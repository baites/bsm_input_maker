INTRODUCTION
============

    top-like Beyond the Standard Model (BSM) analysis Input generator.



COMPILE
=======

    The code only works within the CMSSW framework.

        1. git clone <url>
        2. cd bsm_input_maker
        3. git submodule update --init
        4. cd input
        5. make
        6. cd -
        7. scram b -r -j10



USAGE
=====

    Generate input from PAT:

        1. cd bsm_input_maker/maker/test
        2. dbs search --noheader --query='find file where dataset like XXX' &> input.txt
        3. cmsRun ./input_maker_cfg.py
