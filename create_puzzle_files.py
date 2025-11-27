import argparse

def create_files(id):
	puzzle_name = "puzzle_{:02d}".format(id)
	data = template.replace("<puzzle_id>", puzzle_name)
	try:
		with open("{}.c".format(puzzle_name), 'x') as cfile:
			print('creating new puzzle file ' + puzzle_name)
			cfile.write(data)
	except:
		pass
	open("{}p1.txt".format(puzzle_name), 'a').close()
	open("{}p2.txt".format(puzzle_name), 'a').close()

parser = argparse.ArgumentParser(
	prog="create_puzzle_files",
	description="",
	epilog=""
)
parser.add_argument('id', nargs=1, type=int)
args = parser.parse_args()

template = None
with open('template.c', 'r') as file:
	template = file.read()

if template == None:
	exit(1)

create_files(args.id[0])