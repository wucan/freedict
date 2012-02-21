import os

print "Build freedict ..."

env=Environment()
env.ParseConfig("pkg-config --cflags --libs cspi-1.0 libgnome-2.0 libgnomeui-2.0 gtk+-2.0")

env['CFLAGS'] += ['-g']

SConscript(
	'src/SConscript'
)

env.Default('src')

