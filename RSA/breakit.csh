#!/bin/csh
set user = $1
set Expected = HelloFrom.mdey6.DszSpKatklftLewPxmjdOoJHLrtoQZBwTmTSopVkNRak
set Actual = `./BreakRSA  5002104625374427561 162409060085646275  4573658228876683323 2324556489953957221 630535501010508028 3386475157526168247 1933119509193514798 3920482985182652093 1676913706016319619 265498987128519859 1250796239003581391 1130448145268981087`
echo "expected is $Expected" 
echo "actal    is $Actual"
if ( "$Expected" == "$Actual" ) then
echo "Grade for user $user is 100"
else
echo "Grade for user $user is 50"
endif
