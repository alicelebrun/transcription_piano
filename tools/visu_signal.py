from openturns import *
from openturns.viewer import View
filename = "Lettre_a_Elise.csv"

signal=Sample.ImportFromCSVFile(filename)
g=Graph()
g.setAxes(True)
g.setGrid(True)
g.add(Curve(signal))
g.setTitle("Signal associé à " + filename)
g.setXTitle("t (s)")
g.setYTitle("x(t)")
view = View(g, (1000, 500))
view.save("signal_complet.pdf")
view.close()
