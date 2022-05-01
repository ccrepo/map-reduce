#!/bin/bash

make -f Makefile.client depend ; make -f Makefile.coordinator depend 
\rm *.bak
