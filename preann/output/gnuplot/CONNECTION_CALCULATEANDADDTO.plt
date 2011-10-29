set terminal png 
set output "/home/timon/workspace/preann/output/images/CONNECTION_CALCULATEANDADDTO.png" 
plot  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOFLOAT_C.DAT" using 1:2 title "FLOAT_C" with linespoints lt 1 pt 2 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOFLOAT_SSE2.DAT" using 1:2 title "FLOAT_SSE2" with linespoints lt 2 pt 2 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOFLOAT_CUDA_INV.DAT" using 1:2 title "FLOAT_CUDA_INV" with linespoints lt -1 pt 2 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOBIT_C.DAT" using 1:2 title "BIT_C" with linespoints lt 1 pt 6 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOBIT_SSE2.DAT" using 1:2 title "BIT_SSE2" with linespoints lt 2 pt 6 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOBIT_CUDA_INV.DAT" using 1:2 title "BIT_CUDA_INV" with linespoints lt -1 pt 6 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOSIGN_C.DAT" using 1:2 title "SIGN_C" with linespoints lt 1 pt 4 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOSIGN_SSE2.DAT" using 1:2 title "SIGN_SSE2" with linespoints lt 2 pt 4 ,  "/home/timon/workspace/preann/output/data/CONNECTION_CALCULATEANDADDTOSIGN_CUDA_INV.DAT" using 1:2 title "SIGN_CUDA_INV" with linespoints lt -1 pt 4