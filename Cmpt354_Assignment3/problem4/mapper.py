def mapper (key, val):
	s = []

	try:
		for x in val['subnational']:
			s.append((x['World region'], float(x['MPI Regional'])))
			
	except:
		pass

	

	return s
