import psycopg2
import json

def connect_db():
    conn = psycopg2.connect(
        dbname='postgres',
        user='postgres',
        password='postgres',
        host='localhost',
        port='5432'
    )
    return conn

def export_recipes_to_json():
    conn = connect_db()
    
    
    cur = conn.cursor()
    # cur.execute("SELECT json_agg(recipes) FROM t.recipes;")
    cur.execute("""
        SELECT json_agg(
            json_build_object(
                'id', id,
                'issue_id', issue_id,
                'url', url,
                'title', title,
                'ingredients', (
                    SELECT json_agg(
                        json_build_object(
                            'name', trim(regexp_replace(ingredient, ' - .*', '', 'g')),
                            'quantity', trim(regexp_replace(ingredient, '.* - ([^ ]+).*', '\\1', 'g')),
                            'unit', trim(regexp_replace(ingredient, '.* - [^ ]+ (.*)', '\\1', 'g'))
                        )
                    )
                    FROM unnest(string_to_array(ingredients, ';')) AS ingredient
                ),
                'steps', (
                    SELECT json_agg(trim(step))
                    FROM unnest(string_to_array(steps, ';')) AS step
                ),
                'image_url', image_url,
                'task_lifetime', task_lifetime,
                'wait_time_read_queue', wait_time_read_queue,
                'wait_time_extract_queue', wait_time_extract_queue,
                'wait_time_write_queue', wait_time_write_queue,
                'processing_time_read', processing_time_read,
                'processing_time_extract', processing_time_extract,
                'processing_time_write', processing_time_write
            )
        ) FROM t.recipes;
    """)
    result = cur.fetchone()[0]
    cur.close()
    with open('recipes.json', 'w', encoding='utf-8') as json_file:
        json.dump(result, json_file, ensure_ascii=False, indent=4)

    conn.close()

if __name__ == "__main__":
    export_recipes_to_json()
