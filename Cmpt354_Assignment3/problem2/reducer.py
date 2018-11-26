def reducer (key, list_of_vals):
	return [(key, len(set(list_of_vals)))]