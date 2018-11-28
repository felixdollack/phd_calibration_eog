#!/usr/bin/python

import xml.etree.ElementTree as ET
import sys

# expect a filename as input argument to be used as filename for the output
if (len(sys.argv) < 2):
    print("no filename specified!")
    print("using xx00xx00.xml")
    outfilename = 'xx00xx00.xml'
else:
    tmp = sys.argv[1]
    file_parts = tmp.split('.')
    if (len(file_parts) < 2):
        tmp += '.xml'
    else:
        if (file_parts[1] != 'xml'):
            tmp += '.xml'
    outfilename = tmp

if (len(sys.argv) < 3):
    b_user_vec = False
else:
    b_user_vec = True

positions = dict()
positions['CW']   = list(range(9)) # all CW source positions
positions['CCW']  = list(range(8,-1,-1)) # all CCW source positions
positions['RAND'] = [0, 1, 5, 1, 0, 2, 6, 2, 0, 4, 8, 4, 0, 7, 3, 7, 0] # all rand source positions
commands = ['center.wav','up_left.wav','up.wav','up_right.wav','right.wav',
            'down_right.wav','down.wav','down_left.wav', 'left.wav']

if (b_user_vec):
    inarg = sys.argv[2].split(',')
    vec = []
    vec_len = 0
    for arg in inarg:
        vec.append(positions[arg])
        vec_len += len(positions[arg])
else:
    vec = [positions['CW'], positions['CCW'], positions['RAND']]
    vec_len = len(positions['CW']) + len(positions['CCW']) + len(positions['RAND'])

# lay out structure of xml
root = ET.Element('order')
use_verbal_commands = ET.SubElement(root, 'verbal_commands')
reference_id        = ET.SubElement(root, 'reference')
number_of_points    = ET.SubElement(root, 'size')

use_verbal_commands.text = '1' # use verbal commands
reference_id.text        = '0' # use point 0 (center) as reference
number_of_points.text    = str(vec_len)

# !TODO randomize pattern and add any combination of CW, CCW and rand
for order in vec:
    for item in order:
        elem = ET.SubElement(root,'item')
        nth = ET.SubElement(elem,'n')
        cmd = ET.SubElement(elem,'command')
        nth.text = str(item)
        cmd.text = commands[item]

# write file
mydata = ET.tostring(root).decode()
myfile = open(outfilename, 'w')
myfile.write(mydata)
myfile.close()

print('created file: ', outfilename)
