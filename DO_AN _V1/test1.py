import pyqtgraph as pg
import pyqtgraph.exporters
# generate something to export
plt = pg.plot([1, 5, 2, 4, 3])
# create an exporter instance, as an argument give it
# the item you wish to export
exporter = pg.exporters.ImageExporter(plt.plotItem)
# set export parameters if needed
# (note this also affects height parameter)
exporter.parameters()['width'] = 100
exporter.parameters()['height'] = 100
# save to file
print(exporter)
exporter.export('fileName.png')
