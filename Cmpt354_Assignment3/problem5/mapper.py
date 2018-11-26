def mapper (key, val):
	s = []

	try:
		for x in val['subnational']:
			if float(x['Headcount Ratio Regional']) >= 0.0 and float(x['Headcount Ratio Regional']) < 10.0:
				s.append((('H', 0), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 10.0 and float(x['Headcount Ratio Regional']) < 20.0:
				s.append((('H', 10), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 20.0 and float(x['Headcount Ratio Regional']) < 30.0:
				s.append((('H', 20), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 30.0 and float(x['Headcount Ratio Regional']) < 40.0:
				s.append((('H', 30), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 40.0 and float(x['Headcount Ratio Regional']) < 50.0:
				s.append((('H', 40), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 50.0 and float(x['Headcount Ratio Regional']) < 60.0:
				s.append((('H', 50), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 60.0 and float(x['Headcount Ratio Regional']) < 70.0:
				s.append((('H', 60), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 70.0 and float(x['Headcount Ratio Regional']) < 80.0:
				s.append((('H', 70), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 80.0 and float(x['Headcount Ratio Regional']) < 90.0:
				s.append((('H', 80), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))
			elif float(x['Headcount Ratio Regional']) >= 90.0 and float(x['Headcount Ratio Regional']) < 100.0:
				s.append((('H', 90), (x['Country'], x['Sub-national region'], float(x['Intensity of deprivation Regional']))))

		for x in val['subnational']:
			if float(x['Intensity of deprivation Regional']) >= 0.0 and float(x['Intensity of deprivation Regional']) < 10.0:
				s.append((('I', 0), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 10.0 and float(x['Intensity of deprivation Regional']) < 20.0:
				s.append((('I', 10), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 20.0 and float(x['Intensity of deprivation Regional']) < 30.0:
				s.append((('I', 20), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 30.0 and float(x['Intensity of deprivation Regional']) < 40.0:
				s.append((('I', 30), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 40.0 and float(x['Intensity of deprivation Regional']) < 50.0:
				s.append((('I', 40), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 50.0 and float(x['Intensity of deprivation Regional']) < 60.0:
				s.append((('I', 50), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 60.0 and float(x['Intensity of deprivation Regional']) < 70.0:
				s.append((('I', 60), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 70.0 and float(x['Intensity of deprivation Regional']) < 80.0:
				s.append((('I', 70), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 80.0 and float(x['Intensity of deprivation Regional']) < 90.0:
				s.append((('I', 80), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))
			elif float(x['Intensity of deprivation Regional']) >= 90.0 and float(x['Intensity of deprivation Regional']) < 100.0:
				s.append((('I', 90), (x['Country'], x['Sub-national region'], float(x['Headcount Ratio Regional']))))



			
	except:
		pass
	

	return s




