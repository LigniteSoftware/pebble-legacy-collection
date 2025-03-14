import json


with open('name-to-abbr.json') as json_data1:
    with open('tz-data.json') as json_data2:
        name_iana = json.load(json_data1)
        name_abbr = json.load(json_data2)
        new_name_iana = []

        def in_name_abbr(name):
            is_in = False
            for x in name_abbr:
                if x["fullname"] == name:
                    is_in = True
            return is_in

        def abbr_from_name(name):
            abbr = ''
            for x in name_abbr:
                if x["fullname"] == name:
                    abbr = x["abbreviation"]
            return abbr

        for i, name_iana_pair in enumerate(name_iana):
            print str(len(name_iana) - i) + " entries left"
            iana = name_iana_pair["mapZone"]["_type"]
            name = name_iana_pair["mapZone"]["_other"]
            if in_name_abbr(name):
                new_name_iana.append({
                    "name": name,
                    "iana": iana,
                    "abbr": abbr_from_name(name)
                    })
            else:
                new_abbreviation = raw_input("What abbr is this? " + name)
                name_abbr.append({
                    "abbreviation": new_abbreviation,
                    "fullname": name
                    })
                new_name_iana.append({
                    "name": name,
                    "iana": iana,
                    "abbr": new_abbreviation
                    })

            print "Done, writing to file"
            with open('output.json', 'w') as outfile:
                json.dump(new_name_iana, outfile, indent=2)
