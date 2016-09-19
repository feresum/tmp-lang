import tempfile, os, os.path, sys, shutil, subprocess

def gccCmd(inf, outf):
    return ('g++', '-std=c++11', '-w', inf, '-o', outf)
compileCmd = gccCmd
jpath = os.path.join

if len(sys.argv) == 2:
    infile = sys.stdin
elif len(sys.argv) == 3:
    infile = open(sys.argv[2], 'rb')
else:
    exit(1)


with open(sys.argv[1]) as f:
    prog = f.read()

allowedCh = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ \t\r\n<>,'"
for ch in prog:
    if ch not in allowedCh:
        print 'Illegal char:', ch
        exit(1)

inp = infile.read()
infile.close()

ownpath = os.path.dirname(os.path.realpath(sys.argv[0]))
with open(jpath(ownpath, 'programtemplate.cpp')) as f:
    progt = f.read()
st = ','.join("'\\x%02x'"%ord(c) for c in inp)
cppprog = progt % (jpath(ownpath, 'templates.h'), prog, st)

tmpd = tempfile.mkdtemp()
try:
    progf = jpath(tmpd, 'program.cpp')
    with open(progf, 'w') as f:
        f.write(cppprog)
    exe = jpath(tmpd, 'executable')
    subprocess.check_call(compileCmd(progf, exe))
    subprocess.check_call((exe,))
finally:
    import shutil
    shutil.rmtree(tmpd, ignore_errors=True)
