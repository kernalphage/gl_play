import os
import sys
import capnp

import image_capnp
import time
import cairo

knownCommands = {}


def circle(ctx, values):
	ctx.move_to(values[0] + values[2], values[1]);
	ctx.arc(values[0], values[1], values[2], 0, 6.28);

def translate(ctx, values):
	ctx.translate(values[0], values[1])



knownCommands["c"] = circle;
knownCommands["translate"] = translate

# knownCommands["rotate"] = rotate

def parseCommand(ctx, cmd, values):
	global knownCommands

	if cmd in knownCommands:
		knownCommands[cmd](ctx, values)
	else:
		print("skipping task " + cmd + ", ".join([str(x) for x in values]))


f = open(sys.argv[1], 'r')


surface = cairo.SVGSurface(sys.argv[1] + ".svg", 1000,1000)
context = cairo.Context(surface)


for line in f:
	ll = line.split(" ")
	cmd = ll[0]
	vals = [float(x) * 400 for x in ll[1:]]
	parseCommand(context, cmd, vals)

context.stroke()
