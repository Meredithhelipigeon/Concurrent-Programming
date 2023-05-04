import os

for i in range(120,140):
    os.system('./cardgame 4 d d 1220 >> cardgame.out')
    os.system('./standardCardgame 4 d d 1220 >> standardCardgame.out')
    os.system('diff cardgame.out standardCardgame.out')
    os.system('rm cardgame.out')
    os.system('rm standardCardgame.out')
