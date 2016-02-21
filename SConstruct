
libs = [
    #'/usr/local/lib/libluajit-5.1.a',
    #'/usr/local/lib/libluajit-5.1.dylib',
    #'/usr/local/lib/liblua.dylib',
    #'/usr/local/lib/libglfw3.a',
]

linkflags = [
    '-L/usr/local/lib',
    '-lluajit-5.1',
    '-lglfw3',
    '-pagezero_size 10000', # For LuaJIT
    '-image_base 100000000', # For LuaJIT
    '-framework OpenGL',
    '-framework Cocoa',
    '-framework IOKit',
]
    

env = Environment(CPATH=['src'])
env.VariantDir('build', 'src', duplicate=0)
env.Append(CFLAGS='-g -Wall -Werror -pedantic -ansi -std=c99 -Isrc -O2')
env.Append(LINKFLAGS=' '.join(linkflags))
env.Program('bin/quadrant', env.Glob('src/*.c'))
