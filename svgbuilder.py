import os
import sys
import time
import cairo
import errno



def symlink_force(target, link_name):
    try:
        os.symlink(target, link_name)
    except OSError, e:
        if e.errno == errno.EEXIST:
            os.remove(link_name)
            os.symlink(target, link_name)
        else:
            raise e

knownCommands = {}


def circle(ctx, values):
	ctx.move_to(values[0] + values[2], values[1]);
	ctx.arc(values[0], values[1], values[2], 0, 6.28);

def translate(ctx, values):
	ctx.translate(values[0], values[1])

def stroke(ctx, values):
	ctx.set_source_rgb(*values)

knownCommands["c"] = circle;
knownCommands["translate"] = translate
knownCommands["stroke"] = stroke

# knownCommands["rotate"] = rotate

def parseCommand(ctx, cmd, values):
	global knownCommands

	if cmd in knownCommands:
		knownCommands[cmd](ctx, values)
	else:
		print("skipping task " + cmd + ", ".join([str(x) for x in values]))


f = open(sys.argv[1], 'r')

filename = sys.argv[1] + ".svg";
surface = cairo.SVGSurface(filename, 8.5 * 300, 11 * 300)
context = cairo.Context(surface)


for line in f:
	ll = line.split(" ")
	cmd = ll[0]
	vals = [float(x) for x in ll[1:]]
	parseCommand(context, cmd, vals)

context.stroke()
symlink_force(filename, "latest.svg")
