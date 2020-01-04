text = "we really dont have much except gamers need to rise up against the oakland athletics baseball team they arent gamers oakland was a mistake"

split = text.split( )
toPrint = '{ '
for word in split:
   toPrint += '"' + word.lower( ) + '", '
print( toPrint[:-2] + ' };')
