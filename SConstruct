
libs = [
    #'/usr/local/lib/libluajit-5.1.a',
    '/usr/local/lib/libluajit-5.1.dylib',
    #'/usr/local/lib/liblua.dylib',
    '/usr/local/lib/libglfw.a',
]

linkflags = [
    '-pagezero_size 10000', # For LuaJIT
    '-image_base 100000000', # For LuaJIT
    '-framework OpenGL',
    '-framework Cocoa',
    '-framework IOKit',
]
    

env = Environment(CPATH=['src'])
env.Append(CFLAGS='-g -Wall -Werror -pedantic -std=c99 -Isrc')
env.Append(LINKFLAGS=' '.join(linkflags))
VariantDir('build', 'src', duplicate=0)
src = env.Glob('src/*.c')+env.Glob('src/*.h')+libs
ecoe = env.Program('bin/quadrant', src)
