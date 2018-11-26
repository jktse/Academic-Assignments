def takesecond(elem):
    return elem[1]



def reducer (key, list_of_vals):
	list_of_vals.sort(key = takesecond, reverse = True)
	return [(key, list_of_vals[0])]