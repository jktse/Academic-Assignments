def takethird(elem):
    return elem[2]



def reducer (key, list_of_vals):
	list_of_vals.sort(key = takethird, reverse = True)
	return [(key, list_of_vals[0])]