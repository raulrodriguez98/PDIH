library(tuneR)
library(seewave)
library(audio)

# establecer el path concreto en cada caso a la carpeta de trabajo
setwd("C:/Users/gabri/OneDrive/Escritorio/P5")

# Leer dos ficheros de sonido
nombre <- readWave('nombre.wav')
nombre
listen(nombre)

apellidos <- readWave('apellidos.wav')
apellidos
listen(apellidos)

# Dibujar la forma de onda

plot(extractWave(nombre, from = 1, to = length(nombre)), main="Nombre")
plot(extractWave(apellidos, from = 1, to = length(apellidos)), main="apellidos")

# Unir ambos sonidos
NombreApellidos <- pastew (apellidos, nombre, output="Wave")
NombreApellidos
listen(NombreApellidos)

plot(extractWave(NombreApellidos, from = 1, to = length(NombreApellidos)), main="NombreApellidos")

# Guardar el audio
writeWave (NombreApellidos, file.path("basico.wav"))

# Filtro de frecuencia para eliminar las frecuencias
filtro_NombreApellidos <- bwfilter(NombreApellidos, 48000, channel = 1, n = 1, from = 10000, to = 20000, bandpass = TRUE, listen=FALSE, output = "Wave")
listen(filtro_NombreApellidos)

filtro_NombreApellidos <- normalize(filtro_NombreApellidos, unit = "16")

listen(filtro_NombreApellidos)

writeWave (filtro_NombreApellidos, file.path("filtrado.wav"))


# Añadimo eco:
NombreApellidos_Eco <- echo (NombreApellidos, f=NombreApellidos@samp.rate,amp=c(0.8,0.4,0.2,0.1), delay=c(1,2,3,4), output="Wave")

NombreApellidos_Eco <- normalize(NombreApellidos_Eco, unit = "16")
writeWave (NombreApellidos_Eco, file.path("eco.wav"))


# Al reves:
NombreApellidos_Eco_Alreves <- revw (NombreApellidos_Eco, output="Wave")

NombreApellidos_Eco_Alreves <- normalize(NombreApellidos_Eco_Alreves, unit = "16")
writeWave (NombreApellidos_Eco_Alreves, file.path("alreves.wav"))


