'''
Converts matrices from training into pickle
'''

from pickle import dump
import numpy as np
import pandas as pd


DIRECTORY = 'training/'
FILES = ['majorHighMatrix', 'majorChordMatrix', 'majorLowMatrix', 'minorHighMatrix', 'minorLowMatrix', 'minorChordMatrix']


# Convert all text files into numpy arrays
for f in FILES:
  print('Converting', f)

  temp = pd.read_csv(DIRECTORY+f+'.txt', sep =' ', header=None)

  # the last column is garbage
  temp.drop(temp.columns[len(temp.columns)-1], axis=1, inplace=True)

  with open(f+'.pkl', 'wb') as fout:
    dump(temp.values, fout, protocol=4)

  print('Shape:', temp.values.shape)

print('Done converting all matrices to pickle')
