def mapper (key, val):
	s = []

	try:
		for x in val:
			if x == 'Headcount Ratio Urban':
				if float(val[x]) >= 0.0 and float(val[x]) < 10.0:
					s.append((('H', 0), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 10.0 and float(val[x]) < 20.0:
					s.append((('H', 10), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 20.0 and float(val[x]) < 30.0:
					s.append((('H', 20), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 30.0 and float(val[x]) < 40.0:
					s.append((('H', 30), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 40.0 and float(val[x]) < 50.0:
					s.append((('H', 40), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 50.0 and float(val[x]) < 60.0:
					s.append((('H', 50), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 60.0 and float(val[x]) < 70.0:
					s.append((('H', 60), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 70.0 and float(val[x]) < 80.0:
					s.append((('H', 70), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 80.0 and float(val[x]) < 90.0:
					s.append((('H', 80), (val['Country'], float(val['Intensity of Deprivation Urban']))))
				elif float(val[x]) >= 90.0 and float(val[x]) < 100.0:
					s.append((('H', 90), (val['Country'], float(val['Intensity of Deprivation Urban']))))

		for x in val:
			if x == 'Intensity of Deprivation Urban':
				if float(val[x]) >= 0.0 and float(val[x]) < 10.0:
					s.append((('I', 0), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 10.0 and float(val[x]) < 20.0:
					s.append((('I', 10), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 20.0 and float(val[x]) < 30.0:
					s.append((('I', 20), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 30.0 and float(val[x]) < 40.0:
					s.append((('I', 30), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 40.0 and float(val[x]) < 50.0:
					s.append((('I', 40), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 50.0 and float(val[x]) < 60.0:
					s.append((('I', 50), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 60.0 and float(val[x]) < 70.0:
					s.append((('I', 60), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 70.0 and float(val[x]) < 80.0:
					s.append((('I', 70), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 80.0 and float(val[x]) < 90.0:
					s.append((('I', 80), (val['Country'], float(val['Headcount Ratio Urban']))))
				elif float(val[x]) >= 90.0 and float(val[x]) < 100.0:
					s.append((('I', 90), (val['Country'], float(val['Headcount Ratio Urban']))))


			
	except:
		pass
	

	return s
