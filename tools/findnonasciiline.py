import argparse

parser = argparse.ArgumentParser(description='find non-ascii line and display that')
parser.add_argument('filename', metavar='FILENAME', type=str, help='The name of file to investigate') 

args = parser.parse_args()

fname = args.filename

try:
  with open(fname, 'r', encoding = 'shift_jis') as f:
    lines = f.readlines()
    linenum = 1
    for line in lines:
      try:
        ascii_line = line.encode('ascii') 
      except:
        print("Line " + str(linenum))

      linenum += 1
except:
  print("Error occured while opening " + fname)


