const string HTML_HEADER = R"(
<html><head>
<title>School Report</title>
<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>
<style>
body { font-family: 'Segoe UI', Arial, sans-serif; margin: 0; padding: 40px; background-color: #f0f2f5; }
.container { max-width: 1200px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 15px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
.header { text-align: center; margin-bottom: 40px; padding: 20px; background: linear-gradient(135deg, #1e3c72, #2a5298); color: white; border-radius: 10px; }
.school-name { font-size: 28px; font-weight: bold; margin-bottom: 10px; }
.report-title { font-size: 22px; opacity: 0.9; }
.stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin: 30px 0; }
.stat-card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.05); text-align: center; }
.stat-value { font-size: 24px; font-weight: bold; color: #2a5298; margin: 10px 0; }
.stat-label { color: #666; font-size: 14px; }
table { width: 100%; border-collapse: collapse; margin: 25px 0; font-size: 15px; }
th { background-color: #2a5298; color: white; padding: 15px; text-align: left; }
td { padding: 12px 15px; border-bottom: 1px solid #ddd; }
tr:hover { background-color: #f8f9fa; }
.attendance-bar { height: 20px; background-color: #e9ecef; border-radius: 10px; overflow: hidden; margin: 5px 0; }
.attendance-fill { height: 100%; transition: width 0.5s ease-in-out; }
.chart-container { margin: 30px 0; padding: 20px; background: white; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.05); }
.low { background-color: #dc3545; }
.medium { background-color: #ffc107; }
.high { background-color: #28a745; }
</style>
</head>
)"; 