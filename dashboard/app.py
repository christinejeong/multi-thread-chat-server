from flask import Flask, render_template, jsonify
from datetime import datetime
from monitor import monitor

app = Flask(__name__)

# Start the monitoring system
monitor.start_monitoring()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/stats')
def get_stats():
    # Get real stats from monitor
    stats = monitor.get_stats()
    return jsonify(stats)

@app.route('/api/health')
def health_check():
    return jsonify({'status': 'healthy', 'timestamp': datetime.now().isoformat()})

if __name__ == '__main__':
    print("Starting Chat Server Dashboard on http://localhost:5001")
    app.run(debug=True, host='0.0.0.0', port=5001)