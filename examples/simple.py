import numpy as np
import pykingduck as pkd

a_list = [1, 8, 9.7]

print("A simple list works OK:", a_list)

a_numpy = np.array([1, 8, 9.7])

print("A numpy array works OK:", a_numpy)

numerical_array = pkd.core.NumericalData(a_numpy)

print("A numerical array works OK:", numerical_array.to_list())