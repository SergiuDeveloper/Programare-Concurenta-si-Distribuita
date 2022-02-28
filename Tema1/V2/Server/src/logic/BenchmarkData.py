from threading import Lock


class BenchmarkData:
    __data = {}
    __data_lock = Lock()

    @staticmethod
    def add_data(client_ip, category, timestamp, bytes_count):
        BenchmarkData.__data_lock.acquire()
        if client_ip not in BenchmarkData.__data:
            BenchmarkData.__data[client_ip] = {}
        BenchmarkData.__data[client_ip][category] = (timestamp, bytes_count)
        BenchmarkData.__data_lock.release()

    @staticmethod
    def get_data(client_ip, category):
        BenchmarkData.__data_lock.acquire()
        if client_ip not in BenchmarkData.__data:
            BenchmarkData.__data_lock.release()
            return None
        if category not in BenchmarkData.__data[client_ip]:
            BenchmarkData.__data_lock.release()
            return None
        value = BenchmarkData.__data[client_ip][category]
        BenchmarkData.__data_lock.release()
        return value

    @staticmethod
    def remove_data(client_ip):
        BenchmarkData.__data_lock.acquire()
        if client_ip in BenchmarkData.__data:
            del BenchmarkData.__data[client_ip]
        BenchmarkData.__data_lock.release()
