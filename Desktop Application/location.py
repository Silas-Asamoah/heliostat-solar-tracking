from __future__ import print_function, division

import numpy as np
import astropy.units as u
import matplotlib.pyplot as plt

from astropy.coordinates import SkyCoord

import sunpy.map
from sunpy.coordinates import frames
from sunpy.data.sample import AIA_171_IMAGE


aia = sunpy.map.Map(AIA_171_IMAGE)


stonyhurst_center = SkyCoord(0 * u.deg, 0 * u.deg, frame=frames.HeliographicStonyhurst)


hpc_stonyhurst_center = stonyhurst_center.transform_to(aia.coordinate_frame)
print(hpc_stonyhurst_center)

lon0 = SkyCoord(np.linspace(-80, 80, 100) * u.deg, np.zeros(100) * u.deg, frame=frames.HeliographicStonyhurst)

lat0 = SkyCoord(np.zeros(100) * u.deg, np.linspace(-90, 90, 100)*u.deg,frame=frames.HeliographicStonyhurst)

hpc_lon0 = lon0.transform_to(aia.coordinate_frame)
hpc_lat0 = lat0.transform_to(aia.coordinate_frame)


fig = plt.figure()
ax = plt.subplot(projection=aia)
aia.plot()
ax.plot_coord(hpc_lat0, color="C0")
ax.plot_coord(hpc_lon0, color="C0")
aia.draw_grid()
plt.show()