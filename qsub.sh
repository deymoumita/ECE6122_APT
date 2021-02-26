#!/bin/bash
qsub -X -I -q class -l nodes=1:gpu
echo "qsub -X -I -q class -l nodes=1:gpu"

