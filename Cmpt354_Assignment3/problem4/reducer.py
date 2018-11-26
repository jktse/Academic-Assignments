def reducer (key, list_of_vals):
	return [(key, sum(list_of_vals)/(len(list_of_vals)))]