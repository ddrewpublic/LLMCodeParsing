1. Install `groq`
``` shell
python -m pip install groq
```

1. Add Groq API key to .env
``` shell
echo "GROQ_API_KEY=$GROQ_API_KEY" | tee -a .env
echo ".env" | tee -a .gitignore
```
``` python
# Import variables from .env file
from dotenv import load_dotenv
load_dotenv()
```


