#!/bin/csh
set user = $1
set Expected = HelloFrom.mdey6.obEmrLqxZfIIoMabTmmaIlIgyJKffbwPociKVfXBMLwB
set Actual = `./BreakRSA  1946543471221650659 1169845182547923923  546902500291303227 511230727013836054 305572147512685829 1464647320813687170 1260376261045569404 930549066303834854 1910493932051846590 803230617171448559 1776348653900576709 575638105089048028`
echo "expected is $Expected" 
echo "actal    is $Actual"
if ( "$Expected" == "$Actual" ) then
echo "Grade for user $user is 100"
else
echo "Grade for user $user is 50"
endif
