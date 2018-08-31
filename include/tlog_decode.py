import sys
import struct
import os

def ParseFile(infile,outfile):
    print infile,outfile

    try:
        pfile = open(infile,'rb')
        poutfile = open(outfile,"w+")
    except IOError:
        return
    while True:
        magic = 0
        try:
            magic=ord(pfile.read(1))
        except TypeError:
            break;
        # print magic
        if (magic== 0x06 or magic== 0x07 or magic== 0x08 or magic== 0x09) :
            seq = struct.unpack_from('H',pfile.read(2))[0]
            len = struct.unpack_from('H',pfile.read(2))[0]
            log = pfile.read(len)
            poutfile.write(log)
            # print seq,len,log
            end = pfile.read(1)
        else:
            break
    pfile.close()
    poutfile.flush()
    poutfile.close()




def main(args):
    if 1==len(args) and os.path.isfile(args[0]):
        ParseFile(args[0], args[0]+".log")


if __name__ == "__main__":
    main(sys.argv[1:])