from midiutil.MidiFile import MIDIFile

def nom_vers_indice(nom):
    tous_les_noms = ["La-1", "La-1dièse", "Si-1", "Do0", "Do0dièse", "Ré0", "Ré0dièse", "Mi0", "Fa0", "Fa0dièse", "Sol0", "Sol0dièse", "La0", "La0dièse" "Si0", "Do1", "Do1dièse", "Ré1", "Ré1dièse", "Mi1", "Fa1", "Fa1dièse", "Sol1", "Sol1dièse", "La1", "La1dièse", "Si1", "Do2", "Do2dièse", "Ré2", "Ré2dièse", "Mi2", "Fa2", "Fa2dièse", "Sol2", "Sol2dièse", "La2", "La2dièse", "Si2", "Do3", "Do3dièse", "Ré3", "Ré3dièse", "Mi3", "Fa3", "Fa3dièse", "Sol3", "Sol3dièse", "La3", "La3dièse", "Si3", "Do4", "Do4dièse", "Ré4", "Ré4dièse", "Mi4", "Fa4", "Fa4dièse", "Sol4", "Sol4dièse", "La4", "La4dièse", "Si4", "Do5", "Do5dièse", "Ré5", "Ré5dièse", "Mi5", "Fa5", "Fa5dièse", "Sol5", "Sol5dièse", "La5", "La5dièse", "Si5", "Do6", "Do6dièse", "Ré6", "Ré6dièse", "Mi6", "Fa6", "Fa6dièse", "Sol6", "Sol6dièse", "La6", "La6dièse", "Si6", "Do7"]
    for i in range(len(tous_les_noms)):
        if nom == tous_les_noms[i]:
            return i
    return -1

nom_fichier = "res_vois"
f = open(nom_fichier + ".txt")
lines = f.readlines()

# On cherche l'indice de début de transcription
indice_debut_transcription = 0
for line in lines:
    if len(line)>4 and line[0:4] == "note":
        break
    indice_debut_transcription += 1

# On cherche les quadruplets indice, t_debut, t_fin, duree
indices = list()
t_debut = list()
t_fin = list()
durees = list()
i = indice_debut_transcription
while (len(line) > 4 and line[0:4] == "note"):
    #print("line=", line)
    tokens = line.split()
    nom = tokens[0].split("(")[1]
    index = 21 + nom_vers_indice(nom)
    indices.append(index)
    t1 = int(tokens[1].split("=")[1].split("ms")[0])
    t2 = int(tokens[2].split("=")[1].split("ms")[0])
    duree = t2 - t1
    #print("nom=", nom, "index=", index, "t1=", t1, "t2=", t2, "duree=", duree)
    t_debut.append(int(t1))
    t_fin.append(int(t2))
    durees.append(duree)
    i += 1
    line = lines[i]

duree_tempo = min(durees)
tempo = int(60000.0 / duree_tempo)
print("duree_tempo=", duree_tempo, "ms tempo=", tempo)
t0 = t_debut[0]

# Création en-tête MIDI
mf = MIDIFile(1)     # only 1 track
track = 0   # the only track

time = 0    # start at the beginning
mf.addTrackName(track, time, "Sample Track")
mf.addTempo(track, time, tempo)

channel = 0
volume = 100
for i in range(len(durees)):
    pitch = indices[i]
    time = int(round((t_debut[i] - t0) / duree_tempo))
    duration = int(round(durees[i] / duree_tempo))
    mf.addNote(track, channel, pitch, time, duration, volume)

# write it to disk
with open(nom_fichier + ".mid", 'wb') as outf:
    mf.writeFile(outf)
