import json

with open('output.json') as json_data:
    output = json.load(json_data)
    deduped = []

    def is_new(thing):
        exist = False
        for x in deduped:
            if thing['iana'] == x['iana']:
                exist = True
        return exist

    for i, obj in enumerate(output):
        print str(len(output) - i) + " entries left"
        if not is_new(obj):
            deduped.append(obj)

        print "Done, writing to file"
        with open('deduped.json', 'w') as outfile:
            json.dump(deduped, outfile, indent=2)
