from openturns import *
from openturns.viewer import View
from math import *

H = 3

def calculer_produit_spectral(ind, spectre):
    val = 0.0
    for i in range(1, H+1):
        val += log(spectre[i * ind, 1])
    return val

def maximiser_produit_spectral(spectre):
    indice = 1
    max_val = calculer_produit_spectral(indice, spectre)
    for i in range(2, len(spectre)//H):
        val = calculer_produit_spectral(i, spectre)
        if val > max_val:
            max_val = val
            indice = i
    return spectre[indice, 0], max_val

found_index = False
res = open("res_au_clair.txt")
lines = res.readlines()
for line in lines:
    if len(line) > 22 and line[0:22] == "[transcrire] Maximiser":
        index = int(line.split()[5])
        found_index = True
    if found_index and len(line) > 10 and line[0:10] == "[maximiser":
        found_index = False
        nom = str(index) + "_signal"
        print("Analyse de " + line)
        try:
            tokens = line.split("=")
            touche = tokens[2].split()[0]
            signal = Sample.ImportFromCSVFile("CSV/signal" + str(index) + ".csv")
            g_signal = Graph()
            g_signal.setTitle("Signal temporel")
            g_signal.setXTitle("t (s)")
            g_signal.setYTitle("Amplitude")
            g_signal.setAxes(True)
            g_signal.setGrid(True)
            g_signal.add(Curve(signal))
            view = View(g_signal, (1200, 600))
            view.save(nom + ".png")
            view.close()
        except:
            pass
        nom = str(index) + "_spectrogramme"
        try:
            spectre = Sample.ImportFromCSVFile("CSV/spectrogramme" + str(index) + ".csv")
            FmaxRes = float(tokens[1].split("Hz")[0])
            Fmax, produit = maximiser_produit_spectral(spectre)
            Ftouche = float(tokens[3].split("Hz")[0])
            g = Graph()
            g.setAxes(True)
            g.setGrid(True)
            g.add(Curve(spectre.getMarginal([0,2])[1:len(spectre)//H]))
            P = Sample([[spectre[i, 0], calculer_produit_spectral(i, spectre)] for i in range(1, len(spectre) // H)])
            g.add(Curve(P))
            g.setColors(["red", "blue"])
            ymin = P.getMarginal(1).getMin()[0]
            ymax = P.getMarginal(1).getMax()[0]
            curve = Curve([[Fmax, ymin], [Fmax, ymax]])
            curve.setColor("red")
            curve.setLineStyle("dashed")
            g.add(curve)
            curve = Curve([[Ftouche, ymin], [Ftouche, ymax]])
            curve.setColor("green")
            curve.setLineStyle("dotted")
            g.add(curve)
            g.setTitle("Produit spectral Fmax=" + str(Fmax) + "Hz\n->touche=" + touche + " (F=" + str(Ftouche) + "Hz)")
            g.setXTitle("f (Hz)")
            g.setYTitle("produit")
            g.setLogScale(1)
            view = View(g, (600, 600))
            view.save(str(index) + "_produit_spectral.png")
            view.close()
            g_spectre = Graph()
            g_spectre.setAxes(True)
            g_spectre.setGrid(True)
            g_spectre.setLogScale(3)
            g_spectre.add(Curve(spectre.getMarginal([0,1])[1:]))
            g_spectre.setTitle("Spectrogramme Fmax=" + str(Fmax) + "Hz\n->touche=" + touche + " (F=" + str(Ftouche) + "Hz)")
            g_spectre.setXTitle("f (Hz)")
            g_spectre.setYTitle("Module")
            ymin = spectre.getMarginal(1).getMin()[0]
            ymax = spectre.getMarginal(1).getMax()[0]
            for num in range(1, H+1):
                curve = Curve([[num * Fmax, ymin], [num * Fmax, ymax]])
                curve.setColor("red")
                curve.setLineStyle("dashed")
                g_spectre.add(curve)
            curve = Curve([[Ftouche, ymin], [Ftouche, ymax]])
            curve.setColor("green")
            curve.setLineStyle("dotted")
            g_spectre.add(curve)
            if (abs(Fmax - FmaxRes) > 0.1 * FmaxRes):
                print(nom)
                print("i=", i, "Fmax=", Fmax, "FmaxRes=", FmaxRes, "produit=", produit, "produitRes=", produitRes)
                curve = Curve([[FmaxRes, ymin], [FmaxRes, ymax]])
                curve.setColor("black")
                curve.setLineStyle("dotted")
                g_spectre.add(curve)
            view = View(g_spectre, (600, 600))
            view.save(nom + ".png")
            view.close()
        except:
            pass
