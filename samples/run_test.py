import os
import time

def test_sat():
	work_dir = "./PAsamples/RANDOM-SAT/"
	beg = len("The result: ")
	count = 0
	with open(work_dir+"/0run_result.txt", "w") as fw:
		for parent, dirnames, filenames in os.walk(work_dir):
			for filename in filenames:
				if filename.endswith(".smt2"):
					print(filename)
					test_case = ("%s/%s" % (work_dir, filename))
					time_start = time.time()
					result = os.popen("./testparser " + test_case).readlines()
					time_end = time.time()
					ptime = time_end-time_start
					rs = " "
					for line in result:
						if line.startswith("The result:"):
							rs = line[beg:]
							rs = rs.replace('\n','')
					content = ("%s: %s %s\n" % (filename, rs, ptime))
					fw.write(content)
					count += 1
					# if count > 10:
					#	exit(-1)
					
				


def main():
	test_sat()


if __name__ == '__main__':
	main()
