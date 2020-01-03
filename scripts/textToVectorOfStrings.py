text = "We here at goldman sachs believe that the housing market is stronger than ever Some claim that you should short the very strong housing market Well we think that the housing market has never been stronger in fact at goldman we believe you should buy bonds in the market do not short the housing market please"

split = text.split( )
toPrint = '{ '
for word in split:
   toPrint += '"' + word.lower( ) + '", '
print( toPrint[:-2] + ' };')