import os
from faker import Faker

faker_ru = Faker(locale="ru")
faker_en = Faker(locale="en")

cur_dir = os.path.abspath(os.getcwd()) + "/src/data"
TRANSPORT = "/transport.csv"
COUNT = 1024

VEHICLE_BRANDS = ["Toyota", "Honda", "Ford", "Chevrolet", "BMW", "Mercedes-Benz", "Volkswagen", "Audi",
                "Nissan", "Hyundai", "Kia", "Subaru", "Mazda", "Lexus", "Acura", "Jeep", "Dodge", 
                "Chrysler", "Tesla", "Volvo", "Jaguar", "Land Rover", "Porsche", "Maserati", "Ferrari", 
                "Lamborghini", "Bugatti", "McLaren", "Bentley", "Rolls-Royce", "Peugeot", "Citroën", 
                "Renault", "Fiat", "Alfa Romeo", "Škoda", "MINI", "Mitsubishi", "Suzuki", "Dacia", 
                "Genesis", "Rivian", "Lucid", "Polestar", "Rimac", "Faraday Future", "Byton", 
                "Lynk & Co", "Buick", "Cadillac", "Lincoln", "Infiniti", "Lykan Hypersport", "Koenigsegg",
                "Pagani", "Zenvo", "Hennessey", "Gumpert", "Italdesign Giugiaro", "TVR", "Ultima", "Ascari",
                "Donkervoort", "Spyker", "Saab", "DeSoto", "Hudson", "Nash", "Plymouth", "Pontiac", "Tucker",
                "Cord", "Edsel", "Marmon", "Graham-Paige", "Crosley", "Kaiser-Frazer", "Tucker",
                "De Lorean", "Stutz", "Franklin", "REO", "Stanley", "Packard", "Graham", "Hupmobile",
                "Frazer", "Henry J", "Crosley", "Checker", "Hudson", "Holden", "Borgward", "DeSoto",
                "Edsel", "Graham-Paige", "Kaiser-Frazer", "LaSalle", "Maxwell", "Moon", "Mercer",
                "Marmon", "Ogle", "Packard", "Peerless", "Pierce-Arrow", "Rickenbacker", "Reo",
                "Ruxton", "Studebaker", "Stutz", "Tucker", "Willys", "Apperson", "Case",
                "Cole", "Duesenberg", "Gardner", "Kissel", "Moon", "Monroe", "National", "Overland",
                "Pathfinder", "Playford", "Reo", "Ruxton", "Stanley", "Stoddard-Dayton", "Velie",
                "Winton", "Wills Sainte Claire", "Yates", "Oldsmobile", "Mercury", "Hudson",
                "Kaiser-Frazer", "Crosley", "Nash", "Packard", "Studebaker", "Tucker", "Continental",
                "DeSoto", "Franklin", "Hudson", "Kaiser-Frazer", "Kaiser", "Nash", "Packard",
                "Studebaker", "Tucker", "Willys", "Hudson", "Kaiser-Frazer", "Nash", "Packard",
                "Studebaker", "Tucker", "Willys", "American Bantam", "Graham-Paige", "Graham",
                "Henry J", "Hudson", "Kaiser-Frazer", "Kaiser", "Nash", "Packard", "Studebaker",
                "Willys", "Aston Martin", "Austin", "Austin-Healey", "Bentley", "Bristol", "Daimler",
                "Healey", "Hillman", "Jensen", "Land Rover", "MG", "Morris", "Riley", "Rolls-Royce",
                "Rover", "Singer", "Standard", "Sunbeam", "Triumph", "Vauxhall", "Wolseley"]
