#!/usr/bin/env python
#
# Given path and CRAB job ids find latest files available
# It may turn out that CRAB has created several files with the same
# job ID. Usually this happens if job was resubmitted, e.g.:
#
#   path/file_1_1_ABC.root
#   path/file_1_2_ABC.root
#
# the script will pick the one with latest mtime (modification time)
#
# Created by Samvel Khalatyan, Aug 22, 2011
# Copyright 2011, All rights reserved

import glob
import os
import re
import sys



class Jobs:
    '''Expand CRAB output jobs list into a set of numbers

    CRAB output lists jobs in ranges, e.g.:

        1-4,6,7-8,...

    The Jobs class will convert it into:
        
        1,2,3,4,6,7,8,...'''
    def __init__(self):
        self._jobs = set()

    def add(self, jobs):
        '''Parse list of jobs/job intervals
        
        Method may be called several times. Only unique job IDs will be kept'''
        jobs_list = list(self._jobs)
        for i,v in enumerate(jobs):
            match = re.match("(\d+)\-(\d+)", v)
            if match:
                for k in xrange(int(match.group(1)), int(match.group(2)) + 1):
                    jobs_list.append(k)
            else:
                jobs_list.append(int(v))

        # Store only unique numbers
        #
        self._jobs = set(jobs_list)

    def jobs(self):
        '''Access parsed job IDs'''
        return self._jobs



class Files:
    '''Given job IDs and path search for PAT root files

    Only files with latest modification time will be selected. Sometimes
    CRAB resubmits jobs automatically. This will result in several output files
    with the same job ID'''
    def __init__(self):
        self._files = list()
        self._jobs = None

    def setJobs(self, jobs):
        '''Set parsed jobs object
        
        List of files is automatically emptied'''
        self._files = list()
        self._jobs = jobs;

    def search(self, path):
        '''Search for files with given job IDs at specific path'''
        self._files = list()
        if not self._jobs:
            return

        if not os.path.exists(path):
            return

        jobs = sorted(self._jobs.jobs())

        # enumerate over jobs and search for corresponding latest file
        #
        for i, job in enumerate(jobs):
            pattern = os.path.join(path, "*fat_{0}_*.root".format(job))

            results = glob.iglob(pattern)
            newest_file = ""
            newest_mtime = 0
            try:
                while True:
                    file = results.next()
                    mtime = os.path.getmtime(file)

                    if mtime > newest_mtime:
                        newest_mtime = mtime
                        newest_file = file
            except StopIteration:
                pass

            if not newest_file:
                print "file is not found for job {0}".format(job)

                continue

            self._files.append(newest_file)

    def files(self):
        return self._files



if "__main__" == __name__:
    if 3 > len(sys.argv):
        print "usage: {0} PATH CRAB_JOB_IDS".format(sys.argv[0])

        sys.exit(0)
    else:
        jobs = Jobs()
        for i, v in enumerate(sys.argv[2:]):
            jobs.add(re.split(",", v))

        files = Files()
        files.setJobs(jobs)
        files.search(sys.argv[1])

        print "\n".join(files.files())
