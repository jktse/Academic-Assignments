def mapper (key, val):
	s = []

	try:
		for x in val['subnational']:
			if float(x['MPI National']) >= 0 and float(x['MPI National']) < 0.1:
				s.append((0.0, key))
			elif float(x['MPI National']) >= 0.1 and float(x['MPI National']) < 0.2:
				s.append((0.1, key))
			elif float(x['MPI National']) >= 0.2 and float(x['MPI National']) < 0.3:
				s.append((0.2, key))
			elif float(x['MPI National']) >= 0.3 and float(x['MPI National']) < 0.4:
				s.append((0.3, key))
			elif float(x['MPI National']) >= 0.4 and float(x['MPI National']) < 0.5:
				s.append((0.4, key))
			elif float(x['MPI National']) >= 0.5 and float(x['MPI National']) < 0.6:
				s.append((0.5, key))
			elif float(x['MPI National']) >= 0.6 and float(x['MPI National']) < 0.7:
				s.append((0.6, key))
			elif float(x['MPI National']) >= 0.7 and float(x['MPI National']) < 0.8:
				s.append((0.7, key))
			elif float(x['MPI National']) >= 0.8 and float(x['MPI National']) < 0.9:
				s.append((0.8, key))
			elif float(x['MPI National']) >= 0.9 and float(x['MPI National']) < 1.0:
				s.append((0.9, key))
	except:
		s.append((-1, key))

	

	return s
